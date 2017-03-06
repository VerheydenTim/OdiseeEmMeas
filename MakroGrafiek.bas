Attribute VB_Name = "Grafiek"
Sub GrafiekMaken()
Attribute GrafiekMaken.VB_ProcData.VB_Invoke_Func = " \n14"
'
' GrafiekMaken Macro
'

'
    Range("D2").Select
    ActiveSheet.Shapes.AddChart2(240, xlXYScatterSmoothNoMarkers).Select
    ActiveChart.SetSourceData Source:=Range("DATALOG!$A$1:$G$80308")
    ActiveChart.ChartTitle.Select
    ActiveChart.FullSeriesCollection(1).XValues = "=DATALOG!$G:$G"
    ActiveChart.FullSeriesCollection(1).Values = "=DATALOG!$D:$D"
    ActiveChart.FullSeriesCollection(2).Delete
    ActiveChart.FullSeriesCollection(2).XValues = "=DATALOG!$G$2:$G$80308"
    ActiveChart.FullSeriesCollection(3).Delete
    Range("D4").Select
    ActiveWindow.SmallScroll Down:=-33
    Range("D2").Select
    Cells.Replace What:=".", Replacement:=",", LookAt:=xlPart, SearchOrder _
        :=xlByRows, MatchCase:=False, SearchFormat:=False, ReplaceFormat:=False
    ActiveWindow.SmallScroll Down:=6
    ActiveSheet.ChartObjects("Grafiek 1").Activate
    ActiveChart.PlotArea.Select
    ActiveChart.FullSeriesCollection(2).XValues = "=DATALOG!$G:$G"
    ActiveChart.FullSeriesCollection(2).Values = "=DATALOG!$F:$F"
    ActiveChart.ChartArea.Select
    ActiveSheet.Shapes("Grafiek 1").ScaleWidth 1.4766666667, msoFalse, _
        msoScaleFromTopLeft
    ActiveSheet.Shapes("Grafiek 1").ScaleHeight 1.4069444444, msoFalse, _
        msoScaleFromTopLeft
    ActiveSheet.Shapes("Grafiek 1").ScaleWidth 1.3069977427, msoFalse, _
        msoScaleFromTopLeft
    ActiveSheet.Shapes("Grafiek 1").ScaleHeight 1.1263573544, msoFalse, _
        msoScaleFromTopLeft
    ActiveWindow.SmallScroll Down:=-15
    ActiveChart.Axes(xlCategory).Select
    Range("K36").Select
End Sub
