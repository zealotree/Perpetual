module.exports = [
{
  "type": "toggle",
  "messageKey": "Show_Texture",
  "label": "Display a texture instead of Background Color",
  "defaultValue": false,
},
{
  "type": "toggle",
  "messageKey": "Show_DoW",
  "label": "Display the Week Day Calendar",
  "defaultValue": false,
},
{
  "type": "section",
  "capabilities": ["COLOR"],
  "items": [
    {
    "type": "heading",
    "defaultValue": "Colors"
    },
    {
      "type": "select",
      "messageKey": "SelectedTheme",
      "defaultValue": "perpetual",
      "label": "Theme",
      "options": [
        { 
          "label": "Perpetual", 
          "value": "perpetual" 
        },
        { 
          "label": "Perpetual Dark",
          "value": "dark" 
        },
        { 
          "label": "Custom Theme",
          "value": "custom" 
        }
      ]
    },
    {
    "type": "heading",
    "defaultValue": "Custom Theme"
    },
    {
      "type": "color",
      "messageKey": "BackgroundColor",
      "defaultValue": "0x555555",
      "label": "Background",
      "description": "Background Colour to use when Texture is off."
    },
    {
      "type": "color",
      "messageKey": "DateDotColor",
      "defaultValue": "0x000000",
      "label": "Date Dot Dot",
    },
    {
      "type": "color",
      "messageKey": "CurrentDateDotColor",
      "defaultValue": "0xFFAA00",
      "label": "Current Date Dot",
    },
    {
      "type": "color",
      "messageKey": "AMDotColor",
      "defaultValue": "0xFFFFFF",
      "label": "AM Indicator",
    },
    {
      "type": "color",
      "messageKey": "PMDotColor",
      "defaultValue": "0x000000",
      "label": "PM Indicator",
    },
    {
      "type": "color",
      "messageKey": "MarkerColor",
      "defaultValue": "0xAAAAAA",
      "label": "Marker",
    },
    {
      "type": "color",
      "messageKey": "RingColor",
      "defaultValue": "0xAAAAAA",
      "label": "Ring",
    },
    {
      "type": "color",
      "messageKey": "WeekDayColor",
      "defaultValue": "0x000000",
      "label": "Week Day Dot",
    },
    {
      "type": "color",
      "messageKey": "CurrentWeekDayColor",
      "defaultValue": "0xFFAA00",
      "label": "Current Week Day Dot",
    },
    {
      "type": "color",
      "messageKey": "MonthDotColor",
      "defaultValue": "0xFFAA00",
      "label": "Current Month Dot",
    },
    {
      "type": "color",
      "messageKey": "HourHandColor",
      "defaultValue": "0xFFAA00",
      "label": "Hour Hand",
    },
    {
      "type": "color",
      "messageKey": "MinuteHandColor",
      "defaultValue": "0xFFFFFF",
      "label": "Minute Hand",
    },
    {
      "type": "color",
      "messageKey": "CenterKnobColor",
      "defaultValue": "0xFFFFFF",
      "label": "Center Knob",
    },
    {
      "type": "color",
      "messageKey": "CenterDotColor",
      "defaultValue": "0xAAAAAA",
      "label": "Center Dot",
    }
  ]
},
{
  "type": "submit",
  "defaultValue": "Save"
}
]
