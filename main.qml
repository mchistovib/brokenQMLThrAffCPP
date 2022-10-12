import QtQml
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

Window {
    id: root
    width: 1200
    height: 1000
    visible: true
    title: qsTr("NG-MVP")
    flags: Qt.Window

    ColumnLayout {
        spacing: 2
        Rectangle {
            height: 100
            width: 1200

            Button {
                text: "add rows other thread"
                onClicked: {
                    controller.addOtherThread();
                }
            }
        }
        HorizontalHeaderView {
            id: horizontalHeader
            syncView: tableView
        }
        TableView {
            id: tableView
            model: controller.tableModel
            width: 1200
            height: 900
                   delegate: Rectangle {
                        id: cell
                        implicitWidth: 150
                        implicitHeight: 32
                        property bool selected: false
                        color: Qt.lighter(row % 2 ? "gray" : "darkgray", column % 2 ? 1 : 1.3)

                        RowLayout {
                            id: contentRow
                            visible: true

                            Text {
                                id: columnText
                                width: 150
                                height: 32
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: "white"
                                text: model.display !== undefined ? model.display : ""
                            }
                      }
            }
        }
    }
}
