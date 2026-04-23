$outFile = "resultados_insercion.dat"
"# instancia n coste" | Out-File -Encoding ascii $outFile

Get-ChildItem ".\data\*.tsp" | ForEach-Object {
    $file = $_.FullName
    $instancia = $_.BaseName

    Write-Host "Procesando $instancia con fichero $file"

    $salida = & ".\insercion_economica.exe" "$file"

    $n = ($salida | Select-String "Instancia cargada con").ToString() -replace '.*Instancia cargada con ([0-9]+) ciudades.*','$1'
    $coste = ($salida | Select-String "Coste insercion mas economica").ToString() -replace '.*Coste insercion mas economica: ([0-9]+).*','$1'

    "$instancia $n $coste" | Out-File -Append -Encoding ascii $outFile
}