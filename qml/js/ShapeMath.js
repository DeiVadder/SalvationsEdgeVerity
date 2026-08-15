.pragma library

// Mirrors CalculateSteps::toBaseSymbols() (calculatesteps.cpp) - kept in
// sync manually since plain C++ static methods aren't QML-invokable.
// Symbol values: Dreieck=1, Viereck=2, Kreis=3, Kegel=4, Wuerfel=5,
// Zylinder=6, Pyramide=7, Prisma=8, Kugel=9 (see SymbolEnums 1.0).
function baseSymbolsFor(shape) {
    switch (shape) {
    case 4: return [1, 3]       // Kegel -> Dreieck, Kreis
    case 6: return [3, 2]       // Zylinder -> Kreis, Viereck
    case 8: return [2, 1]       // Prisma -> Viereck, Dreieck
    case 5: return [2, 2]       // Wuerfel -> Viereck, Viereck
    case 7: return [1, 1]       // Pyramide -> Dreieck, Dreieck
    case 9: return [3, 3]       // Kugel -> Kreis, Kreis
    default: return []
    }
}

// Mirrors CalculateSteps::pairToShape() (calculatesteps.cpp).
function shapeForBasePair(a, b) {
    if (a > b) { var t = a; a = b; b = t }
    if (a === 1 && b === 1) return 7  // Dreieck+Dreieck -> Pyramide
    if (a === 1 && b === 2) return 8  // Dreieck+Viereck -> Prisma
    if (a === 1 && b === 3) return 4  // Dreieck+Kreis -> Kegel
    if (a === 2 && b === 2) return 5  // Viereck+Viereck -> Wuerfel
    if (a === 2 && b === 3) return 6  // Viereck+Kreis -> Zylinder
    if (a === 3 && b === 3) return 9  // Kreis+Kreis -> Kugel
    return 0
}

// Given 2 of the 3 outside 3D shapes, returns the base-symbol pair the 3rd
// is forced to be (each of the 3 base symbols appears exactly twice across
// the 3 outside shapes - CalculateSteps::checkIsValid() enforces this).
// Returns [] if the inputs are already inconsistent (over/under-count).
function remainingBasePair(knownShape1, knownShape2) {
    var counts = {1: 0, 2: 0, 3: 0}
    baseSymbolsFor(knownShape1).forEach(function (s) { counts[s]++ })
    baseSymbolsFor(knownShape2).forEach(function (s) { counts[s]++ })

    var remaining = []
    ;[1, 2, 3].forEach(function (s) {
        var need = 2 - counts[s]
        for (var n = 0; n < need; ++n)
            remaining.push(s)
    })
    return remaining.length === 2 ? remaining : []
}
