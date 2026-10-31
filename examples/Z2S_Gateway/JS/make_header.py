import gzip
import re

# 1. Read your hardened custom.js
with open("custom.js", "r", encoding="utf-8") as f:
    js_code = f.read()

# Optional light minification (removes comments and extra whitespace)
js_code = re.sub(r'//.*$', '', js_code, flags=re.MULTILINE)
js_code = re.sub(r'\s+', ' ', js_code)

# 2. Compress with maximum gzip compression (level 9)
compressed = gzip.compress(js_code.encode("utf-8"), compresslevel=9)

# 3. Generate the C++ PROGMEM header file
header_content = f"""// Auto-generated gzipped custom.js PROGMEM header
#pragma once
#include <Arduino.h>

const size_t CUSTOM_JS_GZ_LEN = {len(compressed)};
const uint8_t CUSTOM_JS_GZ[] PROGMEM = {{
"""

hex_bytes = [f"0x{b:02x}" for b in compressed]
for i in range(0, len(hex_bytes), 16):
    header_content += "  " + ", ".join(hex_bytes[i:i+16]) + ",\n"

header_content += "};\n"

with open("custom_js_gz.h", "w", encoding="utf-8") as out:
    out.write(header_content)

print(f"Successfully generated custom_js_gz.h!")
print(f"Original size: {len(js_code)} bytes")
print(f"Gzipped PROGMEM size: {len(compressed)} bytes (fits cleanly in 1 TCP window)")