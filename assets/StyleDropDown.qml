import bb.cascades 1.0

DropDown {
    id: styleDropDown
    objectName: "styleDropDown"
    title: qsTr("Formatting style")
    Option {
        text: "Monokai"
        description: qsTr("This style mimics the Monokai color scheme.")
        value: "monokai"
    }
    Option {
        text: "Manni"
        description: qsTr("A colorful style, inspired by the terminal highlighting style.")
        value: "manni"
    }
    Option {
        text: "rrt"
        description: qsTr("Minimalistic \"rrt\" theme, based on Zap and Emacs defaults.")
        value: "rrt"
    }
    Option {
        text: "perldoc"
        description: qsTr("Style similar to the style used in the perldoc code blocks.")
        value: "perldoc"
    }
    Option {
        text: "Borland"
        description: qsTr("Style similar to the style used in the Borland IDEs.")
        value: "borland"
    }
    Option {
        text: "Colorful"
        description: qsTr("A colorful style, inspired by CodeRay.")
        value: "colorful"
    }
    Option {
        text: "Default"
        description: qsTr("The default style (inspired by Emacs 22).")
        value: "default"
        selected: true
    }
    Option {
        text: "Murphy"
        description: qsTr("Murphy's style from CodeRay.")
        value: "murphy"
    }
    Option {
        text: "vs"
        value: "vs"
    }
    Option {
        text: "Trac"
        description: qsTr("Port of the default trac highlighter design.")
        value: "trac"
    }
    Option {
        text: "Tango"
        description: qsTr("The Crunchy default Style inspired from the color palette from the Tango Icon Theme Guidelines.")
        value: "tango"
    }
    Option {
        text: "Fruity"
        description: qsTr("Pygments version of the \"native\" vim theme.")
        value: "fruity"
    }
    Option {
        text: "Autumn"
        description: qsTr("A colorful style, inspired by the terminal highlighting style.")
        value: "autumn"
    }
    Option {
        text: "bw"
        value: "bw"
    }
    Option {
        text: "Emacs"
        description: qsTr("The default style (inspired by Emacs 22).")
        value: "emacs"
    }
    Option {
        text: "Vim"
        description: qsTr("Styles somewhat like vim 7.0")
        value: "vim"
    }
    Option {
        text: "Pastie"
        description: qsTr("Style similar to the pastie default style.")
        value: "pastie"
    }
    Option {
        text: "Friendly"
        description: qsTr("A modern style based on the VIM pyte theme.")
        value: "friendly"
    }
    Option {
        text: "Native"
        description: qsTr("Pygments version of the \"native\" vim theme.")
        value: "native"
    }
}
