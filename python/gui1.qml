GUI {
    Column {
        width: parent.width
        Slider {
            handle: "sigma"
            decimals: 1
            slider {
                minimumValue: 0
                maximumValue: 4
                stepSize: 0.1
            }

        }
    }
}