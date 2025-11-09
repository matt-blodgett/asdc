pragma Singleton
import QtCore
import QtQuick

QtObject {
    id: root

    property int colorScheme: Qt.Unknown
    property bool darkMode: false

    // onColorSchemeChanged: {
    //     console.log('onColorSchemeChanged')
    //     console.log(root.colorScheme)
    // }

    readonly property Settings styleSettings: Settings {
        category: "style"
        property alias colorScheme: root.colorScheme
    }

    function setColorScheme(toScheme) {
        const systemScheme = Qt.styleHints.colorScheme
        // const currentScheme = root.colorScheme

        // console.log(`toScheme=${toScheme},currentScheme=${currentScheme},systemScheme=${systemScheme},darkMode=${root.darkMode}`)
        console.log(`setColorScheme: toScheme=${toScheme},currentScheme=${root.colorScheme},systemScheme=${systemScheme},darkMode=${root.darkMode}`)

        if (toScheme !== systemScheme) {
            Qt.styleHints.colorScheme = toScheme
        }

        root.colorScheme = toScheme
        root.darkMode = Qt.styleHints.colorScheme === Qt.Dark

        // console.log(`toScheme=${toScheme},currentScheme=${root.colorScheme},systemScheme=${Qt.styleHints.colorScheme},darkMode=${root.darkMode}`)
    }
}
