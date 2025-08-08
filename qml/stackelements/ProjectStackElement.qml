import QtQuick 2.15

import qmlcomponents
import CustomStyle

Item {
    id: elementRoot

    property Project project

    onProjectChanged: console.log(project)
}
