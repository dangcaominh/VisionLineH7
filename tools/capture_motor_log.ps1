param(
    [string]$Port = "COM9",
    [string]$OutputFile = ""
)

$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($OutputFile)) {
    $timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
    $OutputFile = Join-Path ([Environment]::GetFolderPath("Desktop")) "motor-test-$timestamp.csv"
}

$serial = [System.IO.Ports.SerialPort]::new(
    $Port,
    115200,
    [System.IO.Ports.Parity]::None,
    8,
    [System.IO.Ports.StopBits]::One
)
$serial.Handshake = [System.IO.Ports.Handshake]::None
$serial.NewLine = "`n"
$serial.ReadTimeout = 500

$writer = $null

try {
    $serial.Open()
    $writer = [System.IO.StreamWriter]::new(
        $OutputFile,
        $false,
        [System.Text.UTF8Encoding]::new($false)
    )
    $writer.WriteLine("time_ms,duty_percent,delta,total")
    $writer.Flush()
    Write-Host "Reading $Port. CSV: $OutputFile"
    Write-Host "Start the motor test now. Stop the motor with the board button, then press Ctrl+C."

    while ($true) {
        try {
            $line = $serial.ReadLine().TrimEnd("`r")
            Write-Host $line
            if ($line.StartsWith("DATA,") -and -not $line.StartsWith("DATA,time_ms,")) {
                $writer.WriteLine($line.Substring(5))
                $writer.Flush()
            }
        }
        catch [System.TimeoutException] {
            # Keep waiting for the next USB CDC line.
        }
    }
}
finally {
    if ($serial.IsOpen) {
        $serial.Close()
    }
    if ($null -ne $writer) {
        $writer.Dispose()
    }
    Write-Host "CSV saved to: $OutputFile"
}
