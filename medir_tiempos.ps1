$outFile = "tiempos_insercion.dat"
"# instancia n tiempo_medio" | Out-File -Encoding ascii $outFile

Get-ChildItem ".\data\*.tsp" | ForEach-Object {
    $file = $_.FullName
    $instancia = $_.BaseName

    Write-Host "Procesando $instancia..."

    $salidas = @()

    for ($i = 1; $i -le 20; $i++) {
        $tiempo = (Measure-Command { & ".\insercion_economica.exe" "$file" > $null }).TotalSeconds
        $salidas += $tiempo
    }

    $media = ($salidas | Measure-Object -Average).Average
    $n = (& ".\insercion_economica.exe" "$file" | Select-String "Instancia cargada con").ToString() -replace '.*Instancia cargada con ([0-9]+) ciudades.*','$1'

    "$instancia $n $media" | Out-File -Append -Encoding ascii $outFile
}