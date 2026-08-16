.pragma library

// Mirrors CalculateSteps::SymbolTypes (calculatesteps.h). Hardcoded as plain
// ints rather than importing the SymbolEnums QML module, since .pragma
// library scripts can't cleanly reference a C++-registered enum type. Keep
// in sync if the enum ever changes.
function iconSource(symbol) {
    switch (symbol) {
    case 1: return "qrc:/images/svg/dreieck.svg" // Dreieck
    case 2: return "qrc:/images/svg/viereck.svg" // Viereck
    case 3: return "qrc:/images/svg/kreis.svg" // Kreis
    case 4: return "qrc:/images/svg/kegel.svg" // Kegel
    case 5: return "qrc:/images/svg/wuerfel.svg" // Wuerfel
    case 6: return "qrc:/images/svg/zylinder.svg" // Zylinder
    case 7: return "qrc:/images/svg/pyramide.svg" // Pyramide
    case 8: return "qrc:/images/svg/prisma.svg" // Prisma
    case 9: return "qrc:/images/svg/kugel.svg" // Kugel
    default: return ""
    }
}

function shapeName(symbol) {
    switch (symbol) {
    case 1: return qsTr("Triangle")
    case 2: return qsTr("Square")
    case 3: return qsTr("Circle")
    case 4: return qsTr("Cone")
    case 5: return qsTr("Cube")
    case 6: return qsTr("Cylinder")
    case 7: return qsTr("Pyramid")
    case 8: return qsTr("Prism")
    case 9: return qsTr("Sphere")
    default: return ""
    }
}

function is2dSymbol(symbol) {
    return symbol === 1 || symbol === 2 || symbol === 3
}

function is3dShape(symbol) {
    return symbol >= 4 && symbol <= 9
}
