using namespace System.Collections.Generic
# Paths
$inputFile = ".\examples\Z2S_Gateway\z2s_devices_database.h"
$versionFile = ".\examples\Z2S_Gateway\z2s_version_info.h"
$finalFile = ".\supported-devices.md"
$version = "1.0.0"

# 1. get version from the file
$versionContent = Get-Content $versionFile -Raw
if ($versionContent -match '#define\s+Z2S_VERSION\s+"([^"]+)"') {
    $version = $Matches[1] 
}
Write-Output $version

# 2. Read the z2s_devices_database.h file and extract the array between //DEVICES_START and //DEVICES_END

# Field names from z2s_device_entity_t
$fields = @("manufacturer_name", "model_name", "z2s_device_uid", "z2s_device_desc_id", "z2s_device_endpoints_count", "z2s_device_endpoints")

# Read all lines
$lines = Get-Content $inputFile

# Find markers
$startIndex = ($lines | Select-String "//DEVICES_START").LineNumber
$endIndex = ($lines | Select-String "//DEVICES_END").LineNumber

$content = ""
if ($startIndex -and $endIndex -and $endIndex -gt $startIndex) {
    $tableLines = $lines[($startIndex + 2)..($endIndex - 2)]
    $content = $tableLines -join "`n"
}

# 3. Parse each entry to extract fields

$matches = [regex]::Matches($content, '(?s)\{.*?\}')
Write-Output $matches.Count

$fields = @("manufacturer_name", "model_name", "z2s_device_desc_id")
$objArray = [List[psobject]]::new();
# objArray looks like this
# [
#   { 
#     "manufacturer_name": "XYZ", 
#     "model_name": "ABC", 
#     "z2s_device_desc_id": "Z2S_DEVICE_DESC_123" 
#   },
# ]
foreach ($match in $matches) {
    $entry = $match.Value
    $obj = @{}
    foreach ($field in $fields) {
        $pattern = "\.$field\s*=\s*([^\n,}]+)"
        if ($entry -match $pattern) {
            $value = $matches[1].Trim()
            if ($value.StartsWith('"') -and $value.EndsWith('"')) {
                $value = $value.Trim('"')
            }
            $obj[$field] = $value
        }
    }

    $objArray.Add($obj);
}

# 4. Export to Markdown table
Write-Output $version
$md = "# List of current supported devices.`r`n`r`n"
$md += "State for version: $version`r`n`r`n"
$md += "When `Model name` is empty it is how device reports its name. `r`n`r`n"
$md += "| **Model name** | **Manufacturer_ name** | **Z2S Device Desc ID** |`r`n"
$md += "|------------|------------|-----------|`r`n"

foreach ($row in $objArray) {
# naive filtering duplicates from many endpoints entries
 if($row["z2s_device_desc_id"] -ne ""  -and $null -ne $row["z2s_device_desc_id"]) {
    $md += "| " + $row["model_name"] + " | " + $row["manufacturer_name"] + " | " + $row["z2s_device_desc_id"] + " | `r`n"
 }
}

Set-Content -Path $finalFile -Value $md
Write-Host "Markdown table saved to finalFile"