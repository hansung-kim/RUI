object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'ADS-B Viewer'
  ClientHeight = 1041
  ClientWidth = 1228
  Color = clBtnFace
  Constraints.MinHeight = 740
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'Tahoma'
  Font.Style = [fsBold]
  Menu = MainMenu1
  Position = poScreenCenter
  WindowState = wsMaximized
  OnClose = FormClose
  OnCreate = FormCreate
  OnMouseWheel = FormMouseWheel
  TextHeight = 12
  object Label16: TLabel
    Left = 13
    Top = 67
    Width = 31
    Height = 13
    Caption = 'ICAO:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label17: TLabel
    Left = 48
    Top = 67
    Width = 46
    Height = 13
    Caption = 'No Hook'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label23: TLabel
    Left = 13
    Top = 51
    Width = 80
    Height = 13
    Caption = 'Wi-Fi Status :  '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label24: TLabel
    Left = 92
    Top = 51
    Width = 21
    Height = 13
    Caption = 'N/A'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label25: TLabel
    Left = 74
    Top = 70
    Width = 21
    Height = 13
    Caption = 'N/A'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label26: TLabel
    Left = 13
    Top = 70
    Width = 55
    Height = 13
    Caption = 'FLIGHT #:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object RightPanel: TPanel
    Left = 978
    Top = 0
    Width = 250
    Height = 1041
    Align = alRight
    Color = clWindow
    Constraints.MaxWidth = 250
    TabOrder = 0
    object Panel1: TPanel
      Left = 1
      Top = 856
      Width = 248
      Height = 184
      Align = alBottom
      TabOrder = 0
      object Label12: TLabel
        Left = 5
        Top = 16
        Width = 55
        Height = 12
        Caption = 'CPA TIME:'
      end
      object Label19: TLabel
        Left = 5
        Top = 31
        Width = 82
        Height = 12
        Caption = 'CPA DISTANCE:'
      end
      object CpaTimeValue: TLabel
        Left = 66
        Top = 16
        Width = 28
        Height = 12
        Caption = 'NONE'
      end
      object CpaDistanceValue: TLabel
        Left = 93
        Top = 31
        Width = 25
        Height = 12
        Caption = 'None'
      end
      object Label22: TLabel
        Left = 8
        Top = 136
        Width = 103
        Height = 12
        Caption = ' Playback Speed:    x'
      end
      object ZoomIn: TButton
        Left = 5
        Top = 110
        Width = 65
        Height = 18
        Caption = 'Zoom In'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 0
        OnClick = ZoomInClick
      end
      object ZoomOut: TButton
        Left = 160
        Top = 110
        Width = 65
        Height = 18
        Caption = 'Zoom Out'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        OnClick = ZoomOutClick
      end
      object TimeToGoTrackBar: TTrackBar
        Left = 5
        Top = 71
        Width = 228
        Height = 33
        Max = 1800
        Min = 1
        Frequency = 100
        Position = 1
        TabOrder = 3
        OnChange = TimeToGoTrackBarChange
      end
      object TimeToGoCheckBox: TCheckBox
        Left = 5
        Top = 44
        Width = 105
        Height = 27
        Caption = 'Time-To-Go'
        Checked = True
        State = cbChecked
        TabOrder = 4
      end
      object TimeToGoText: TStaticText
        Left = 114
        Top = 51
        Width = 76
        Height = 16
        Caption = '00:00:00:000'
        TabOrder = 5
      end
      object TrackBarPlaybackSpeed: TTrackBar
        Left = 5
        Top = 154
        Width = 228
        Height = 33
        Max = 100
        Min = 1
        Frequency = 10
        Position = 1
        TabOrder = 1
        OnChange = PlaybackSpeedTrackBarChange
      end
      object PlaybackSpeedText: TStaticText
        Left = 114
        Top = 136
        Width = 11
        Height = 16
        Caption = '1'
        TabOrder = 6
      end
    end
    object Panel3: TPanel
      Left = 1
      Top = 1
      Width = 248
      Height = 855
      Align = alClient
      Constraints.MinHeight = 70
      TabOrder = 1
      ExplicitHeight = 822
      object Label15: TLabel
        Left = 72
        Top = 501
        Width = 98
        Height = 13
        Caption = 'Areas Of Interest'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object DrawMap: TCheckBox
        Left = 5
        Top = 311
        Width = 98
        Height = 17
        Caption = 'Display Map'
        Checked = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        State = cbChecked
        TabOrder = 0
      end
      object PurgeStale: TCheckBox
        Left = 5
        Top = 334
        Width = 96
        Height = 19
        Caption = 'Purge Stale'
        Checked = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        State = cbChecked
        TabOrder = 1
      end
      object CSpinStaleTime: TCSpinEdit
        Left = 107
        Top = 313
        Width = 62
        Height = 21
        Increment = 5
        MaxValue = 1000
        MinValue = 5
        TabOrder = 2
        Value = 90
      end
      object PurgeButton: TButton
        Left = 186
        Top = 316
        Width = 51
        Height = 16
        Caption = 'Purge'
        TabOrder = 3
        OnClick = PurgeButtonClick
      end
      object AreaListView: TListView
        Left = 5
        Top = 520
        Width = 235
        Height = 97
        Columns = <
          item
            Caption = 'Area'
            Width = 170
          end
          item
            Caption = 'Color'
            Width = 40
          end>
        MultiSelect = True
        ReadOnly = True
        RowSelect = True
        TabOrder = 4
        ViewStyle = vsReport
        OnCustomDrawItem = AreaListViewCustomDrawItem
        OnSelectItem = AreaListViewSelectItem
      end
      object Insert: TButton
        Left = 13
        Top = 623
        Width = 36
        Height = 18
        Caption = 'Insert'
        TabOrder = 5
        OnClick = InsertClick
      end
      object Delete: TButton
        Left = 166
        Top = 623
        Width = 44
        Height = 18
        Caption = 'Delete'
        Enabled = False
        TabOrder = 6
        OnClick = DeleteClick
      end
      object Complete: TButton
        Left = 53
        Top = 623
        Width = 57
        Height = 18
        Caption = 'Complete'
        Enabled = False
        TabOrder = 7
        OnClick = CompleteClick
      end
      object Cancel: TButton
        Left = 115
        Top = 623
        Width = 41
        Height = 18
        Caption = 'Cancel'
        Enabled = False
        TabOrder = 8
        OnClick = CancelClick
      end
      object RawConnectButton: TButton
        Left = 5
        Top = 405
        Width = 104
        Height = 18
        Caption = 'Raw Connect'
        TabOrder = 9
        OnClick = RawConnectButtonClick
      end
      object RawIpAddress: TEdit
        Left = 115
        Top = 405
        Width = 110
        Height = 20
        TabOrder = 10
        Text = '127.0.0.1'
      end
      object CycleImages: TCheckBox
        Left = 107
        Top = 334
        Width = 96
        Height = 19
        Caption = 'Cycle Images'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 11
        OnClick = CycleImagesClick
      end
      object Panel4: TPanel
        Left = 5
        Top = 76
        Width = 244
        Height = 229
        BevelInner = bvLowered
        BevelKind = bkFlat
        TabOrder = 12
        object CLatLabel: TLabel
          Left = 66
          Top = 60
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object CLonLabel: TLabel
          Left = 66
          Top = 79
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          Transparent = False
        end
        object SpdLabel: TLabel
          Left = 66
          Top = 95
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object HdgLabel: TLabel
          Left = 66
          Top = 114
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object AltLabel: TLabel
          Left = 66
          Top = 133
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object MsgCntLabel: TLabel
          Left = 66
          Top = 152
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object TrkLastUpdateTimeLabel: TLabel
          Left = 120
          Top = 206
          Width = 72
          Height = 13
          Caption = '00:00:00:000'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label14: TLabel
          Left = 5
          Top = 206
          Width = 109
          Height = 13
          Caption = 'LAST UPDATE TIME:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label13: TLabel
          Left = 5
          Top = 153
          Width = 52
          Height = 13
          Caption = 'MSG CNT:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label10: TLabel
          Left = 5
          Top = 134
          Width = 24
          Height = 13
          Caption = 'ALT:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label9: TLabel
          Left = 5
          Top = 115
          Width = 27
          Height = 13
          Caption = 'HDG:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label8: TLabel
          Left = 5
          Top = 96
          Width = 25
          Height = 13
          Caption = 'SPD:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label7: TLabel
          Left = 5
          Top = 79
          Width = 24
          Height = 13
          Caption = 'LON:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label6: TLabel
          Left = 5
          Top = 60
          Width = 24
          Height = 13
          Caption = 'LAT:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label18: TLabel
          Left = 5
          Top = 41
          Width = 55
          Height = 13
          Caption = 'FLIGHT #:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object FlightNumLabel: TLabel
          Left = 66
          Top = 41
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object ICAOLabel: TLabel
          Left = 66
          Top = 22
          Width = 46
          Height = 13
          Caption = 'No Hook'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label5: TLabel
          Left = 5
          Top = 22
          Width = 31
          Height = 13
          Caption = 'ICAO:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label4: TLabel
          Left = 75
          Top = 0
          Width = 85
          Height = 16
          Caption = 'Close Control'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = [fsBold, fsUnderline]
          ParentFont = False
        end
        object Label20: TLabel
          Left = 5
          Top = 172
          Width = 53
          Height = 13
          Caption = 'DEP-ARR:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object FlightDepArrLabel: TLabel
          Left = 17
          Top = 189
          Width = 21
          Height = 13
          Caption = 'N/A'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
      end
      object Panel5: TPanel
        Left = 5
        Top = 2
        Width = 244
        Height = 77
        BevelInner = bvLowered
        BevelKind = bkFlat
        TabOrder = 13
        object Lon: TLabel
          Left = 31
          Top = 40
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label3: TLabel
          Left = 5
          Top = 40
          Width = 23
          Height = 13
          Caption = 'Lon:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Lat: TLabel
          Left = 31
          Top = 21
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label2: TLabel
          Left = 5
          Top = 21
          Width = 21
          Height = 13
          Caption = 'Lat:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object SystemTimeLabel: TLabel
          Left = 5
          Top = 4
          Width = 74
          Height = 13
          Caption = 'System Time'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object ViewableAircraftCountLabel: TLabel
          Left = 157
          Top = 59
          Width = 7
          Height = 12
          Caption = '0'
        end
        object AircraftCountLabel: TLabel
          Left = 66
          Top = 58
          Width = 7
          Height = 13
          Caption = '0'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label11: TLabel
          Left = 101
          Top = 58
          Width = 53
          Height = 13
          Caption = 'Viewable:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label1: TLabel
          Left = 5
          Top = 58
          Width = 58
          Height = 13
          Caption = '# Aircraft:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object SystemTime: TStaticText
          Left = 85
          Top = 4
          Width = 76
          Height = 17
          Caption = '00:00:00:000'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
        end
      end
      object RawPlaybackButton: TButton
        Left = 115
        Top = 429
        Width = 96
        Height = 17
        Caption = 'Raw Playback'
        TabOrder = 14
        OnClick = RawPlaybackButtonClick
      end
      object RawRecordButton: TButton
        Left = 5
        Top = 429
        Width = 104
        Height = 17
        Caption = 'Raw Record'
        TabOrder = 15
        OnClick = RawRecordButtonClick
      end
      object SBSConnectButton: TButton
        Left = 5
        Top = 452
        Width = 104
        Height = 20
        Caption = 'SBS Connect'
        TabOrder = 16
        OnClick = SBSConnectButtonClick
      end
      object SBSIpAddress: TEdit
        Left = 115
        Top = 452
        Width = 117
        Height = 20
        TabOrder = 17
        Text = 'data.adsbhub.org'
      end
      object SBSRecordButton: TButton
        Left = 5
        Top = 478
        Width = 104
        Height = 17
        Caption = 'SBS Record'
        TabOrder = 18
        OnClick = SBSRecordButtonClick
      end
      object SBSPlaybackButton: TButton
        Left = 115
        Top = 478
        Width = 96
        Height = 17
        Caption = 'SBS Playback'
        TabOrder = 19
        OnClick = SBSPlaybackButtonClick
      end
      object DeadReckonLabel: TCheckBox
        Left = 5
        Top = 359
        Width = 121
        Height = 19
        Caption = 'Dead Reckoning'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 20
        OnClick = CycleImagesClick
      end
      object CReckoningTime: TCSpinEdit
        Left = 132
        Top = 357
        Width = 62
        Height = 21
        MaxValue = 60
        MinValue = 1
        TabOrder = 21
        Value = 10
      end
      object ColorAltitudeLabel: TCheckBox
        Left = 5
        Top = 380
        Width = 140
        Height = 19
        Caption = 'Color-code Altitude'
        Checked = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        State = cbChecked
        TabOrder = 22
        OnClick = CycleImagesClick
      end
    end
    object Panel2: TPanel
      Left = 1
      Top = 720
      Width = 248
      Height = 107
      TabOrder = 2
      object BigFromTimeLabel: TLabel
        Left = 5
        Top = 63
        Width = 69
        Height = 13
        Caption = 'From Time:  '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object BigToTimeLabel: TLabel
        Left = 5
        Top = 87
        Width = 54
        Height = 13
        Caption = 'To Time:  '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object MapComboBox: TComboBox
        Left = 5
        Top = 6
        Width = 100
        Height = 20
        Style = csDropDownList
        TabOrder = 0
        OnChange = MapComboBoxChange
        Items.Strings = (
          'Google Maps'
          'VFR'
          'IFS Low'
          'IFS High'
          'OpenStreetMap')
      end
      object BigQueryCheckBox: TCheckBox
        Left = 121
        Top = 7
        Width = 124
        Height = 17
        Caption = 'BigQuery Upload'
        TabOrder = 1
        OnClick = BigQueryCheckBoxClick
      end
      object BigQueryPlayback: TButton
        Left = 5
        Top = 30
        Width = 196
        Height = 25
        Caption = 'BigQuery Playback'
        TabOrder = 2
        OnClick = BigQueryPlaybackClick
      end
      object BigFromDateTimePicker: TDateTimePicker
        Left = 73
        Top = 62
        Width = 167
        Height = 19
        Date = 45834.000000000000000000
        Time = 0.533333333332848300
        Kind = dtkDateTime
        MaxDate = 45869.999988425920000000
        MinDate = 45809.000000000000000000
        TabOrder = 3
      end
      object BigToDateTimePicker: TDateTimePicker
        Left = 73
        Top = 84
        Width = 167
        Height = 20
        Date = 45834.000000000000000000
        Time = 0.534722222218988500
        Kind = dtkDateTime
        MaxDate = 45869.999988425920000000
        MinDate = 45809.000000000000000000
        TabOrder = 4
      end
    end
    object Panel6: TPanel
      Left = 1
      Top = 659
      Width = 248
      Height = 60
      Align = alCustom
      TabOrder = 3
      object Label21: TLabel
        Left = 83
        Top = 2
        Width = 87
        Height = 16
        Caption = 'Flight Tracker'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold, fsUnderline]
        ParentFont = False
      end
      object WiFiStatus: TLabel
        Left = 5
        Top = 24
        Width = 115
        Height = 13
        Caption = 'Connection Status :  '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object WiFiStatusText: TLabel
        Left = 126
        Top = 24
        Width = 21
        Height = 13
        Caption = 'N/A'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object SDRStatus: TLabel
        Left = 5
        Top = 40
        Width = 75
        Height = 13
        Caption = 'SDR Status :  '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object SDRStatusText: TLabel
        Left = 91
        Top = 40
        Width = 21
        Height = 13
        Caption = 'N/A'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
    end
  end
  object ObjectDisplay: TOpenGLPanel
    Left = 0
    Top = 0
    Width = 978
    Height = 1041
    Cursor = crCross
    Align = alClient
    TabOrder = 1
    PFDFlags = [f_PFD_DRAW_TO_WINDOW, f_PFD_SUPPORT_OPENGL, f_PFD_DOUBLEBUFFER]
    Font3D_Type.Charset = ANSI_CHARSET
    Font3D_Type.Color = clWindowText
    Font3D_Type.Height = -9
    Font3D_Type.Name = 'Arial'
    Font3D_Type.Style = []
    Font3D_Extrustion = 0.100000001490116100
    Font2D_Enabled = True
    Font2D_Type.Charset = ANSI_CHARSET
    Font2D_Type.Color = clWindowText
    Font2D_Type.Height = -27
    Font2D_Type.Name = 'Arial'
    Font2D_Type.Style = [fsBold]
    OnMouseDown = ObjectDisplayMouseDown
    OnMouseMove = ObjectDisplayMouseMove
    OnMouseUp = ObjectDisplayMouseUp
    OnInit = ObjectDisplayInit
    OnResize = ObjectDisplayResize
    OnPaint = ObjectDisplayPaint
  end
  object MainMenu1: TMainMenu
    Left = 24
    object File1: TMenuItem
      Caption = 'File'
      object UseSBSRemote: TMenuItem
        Caption = 'ADS-B Hub'
        OnClick = UseSBSRemoteClick
      end
      object UseSBSLocal: TMenuItem
        Caption = 'ADS-B Local'
        OnClick = UseSBSLocalClick
      end
      object LoadARTCCBoundaries1: TMenuItem
        Caption = 'Load ARTCC Boundaries'
        OnClick = LoadARTCCBoundaries1Click
      end
      object Exit1: TMenuItem
        Caption = 'Exit'
        OnClick = Exit1Click
      end
    end
  end
  object Timer1: TTimer
    Interval = 500
    OnTimer = Timer1Timer
    Left = 88
  end
  object Timer2: TTimer
    Interval = 5000
    OnTimer = Timer2Timer
    Left = 136
  end
  object IdTCPClientRaw: TIdTCPClient
    OnDisconnected = IdTCPClientRawDisconnected
    OnConnected = IdTCPClientRawConnected
    ConnectTimeout = 0
    Port = 0
    ReadTimeout = -1
    ReuseSocket = rsTrue
    Left = 208
  end
  object RecordRawSaveDialog: TSaveDialog
    DefaultExt = 'raw'
    Filter = 'raw|*.raw'
    Left = 328
  end
  object PlaybackRawDialog: TOpenDialog
    DefaultExt = 'raw'
    Filter = 'raw|*.raw'
    Left = 448
  end
  object IdTCPClientSBS: TIdTCPClient
    OnDisconnected = IdTCPClientSBSDisconnected
    OnConnected = IdTCPClientSBSConnected
    ConnectTimeout = 0
    Port = 0
    ReadTimeout = -1
    ReuseSocket = rsTrue
    Left = 552
  end
  object RecordSBSSaveDialog: TSaveDialog
    DefaultExt = 'sbs'
    Filter = 'sbs|*.sbs'
    Left = 664
  end
  object PlaybackSBSDialog: TOpenDialog
    DefaultExt = 'sbs'
    Filter = 'sbs|*.sbs'
    Left = 784
  end
  object IdUDPServer1: TIdUDPServer
    Bindings = <>
    DefaultPort = 0
    OnUDPRead = IdUDPServer1UDPRead
    Left = 40
    Top = 72
  end
end
