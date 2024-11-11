import QtQuick 2.15
import QtLocation 5.15
import QtPositioning 5.15

Rectangle {
    width: 800
    height: 600
    color: "transparent" // Transparent background

    Map {
        anchors.fill: parent
        plugin: Plugin { name: "osm" } // You may need to configure this based on the map provider

        // Set the center to the midpoint of the bounding box
        center: QtPositioning.coordinate((47.74453 + 47.75026) / 2, (7.32325 + 7.35230) / 2)

        // Adjust zoom level to fit the bounding box area
        zoomLevel: 16

        // Optional: Customize markers or overlays as needed
    }
}
