Attribute VB_Name = "MacroBewerkingExcel"
Sub ExcelBewerking()
Attribute ExcelBewerking.VB_ProcData.VB_Invoke_Func = " \n14"
'
' ExcelBewerking Macro
'

'
    Sheets.Add After:=ActiveSheet
    Sheets("Blad1").Select
    Sheets("Blad1").Name = "Info"
    Range("A2").Select
    ActiveCell.FormulaR1C1 = "Startuur"
    Range("A3").Select
    ActiveCell.FormulaR1C1 = "Startminuten"
    Range("B2").Select
    ActiveCell.FormulaR1C1 = "16"
    Range("B3").Select
    ActiveCell.FormulaR1C1 = "4"
    Range("C3").Select
    Columns("A:A").EntireColumn.AutoFit
    Range("A6").Select
    ActiveCell.FormulaR1C1 = "Omgerekend naar ms"
    Range("B6").Select
    ActiveCell.FormulaR1C1 = "=R[-4]C*60*60*1000+R[-3]C*60*1000"
    Range("B6").Select
    Selection.Copy
    Sheets("DATALOG").Select
    Range("G1").Select
    Application.CutCopyMode = False
    ActiveCell.FormulaR1C1 = "=RC[-5]+Info!R[5]C[-5]"
    Selection.NumberFormat = "[$-x-systime]h:mm:ss AM/PM"
    Rows("1:1").Select
    Selection.Insert Shift:=xlDown, CopyOrigin:=xlFormatFromLeftOrAbove
    Range("G1").Select
    ActiveCell.FormulaR1C1 = "Uur"
    Range("G2").Select
    ActiveCell.FormulaR1C1 = "=(RC[-5]+Info!R6C2)/86400000"
    Range("G2").Select
    Selection.AutoFill Destination:=Range("G2:G80308"), Type:=xlFillDefault
   
End Sub
