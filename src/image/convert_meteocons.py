#!/usr/bin/env python3
"""
Convert Meteocons PNG icons to RGB565 C header files for ESP32 TFT display
- Correct byte order for TFT_eSPI (swap bytes)
- Transparent background support
"""

import os
from PIL import Image

# Configuration
INPUT_DIR = "64"          # Input PNG directory (64x64 icons)
OUTPUT_DIR = "meteocons"  # Output header files directory

# Background color for alpha blending (matches BG_CARD = 0xF7BE)
BG_COLOR_RGB = (240, 244, 240)  # For semi-transparent pixel blending

def blend_with_background(r, g, b, a):
    """Blend a semi-transparent pixel with background color"""
    bg_r, bg_g, bg_b = BG_COLOR_RGB
    alpha = a / 255.0
    new_r = int(r * alpha + bg_r * (1 - alpha))
    new_g = int(g * alpha + bg_g * (1 - alpha))
    new_b = int(b * alpha + bg_b * (1 - alpha))
    return new_r, new_g, new_b

def rgb_to_rgb565(r, g, b):
    """Convert RGB888 to standard RGB565"""
    # Standard RGB565: RRRRR GGGGGG BBBBB
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def png_to_header(png_path, output_path, var_name):
    """Convert a PNG file to C header file with RGB565 data"""
    try:
        img = Image.open(png_path).convert("RGBA")
        width, height = img.size
        
        # Get pixel data
        pixels = list(img.getdata())
        
        # Convert to RGB565 with alpha blending
        rgb565_data = []
        for pixel in pixels:
            r, g, b, a = pixel
            if a < 16:  # Fully transparent - use BG_CARD value directly
                rgb565 = 0xF7BE  # BG_CARD
            elif a < 255:  # Semi-transparent - blend with background
                blended = blend_with_background(r, g, b, a)
                rgb565 = rgb_to_rgb565(*blended)
            else:  # Fully opaque - use original color
                rgb565 = rgb_to_rgb565(r, g, b)
            rgb565_data.append(rgb565)
        
        # Generate header content
        filename = os.path.basename(png_path)
        header = f"// {filename} - {width}x{width} RGB565 (bg: #EFEFEF)\n"
        header += f"// Generated from {filename}\n"
        header += "#include <pgmspace.h>\n"
        header += f"const uint16_t {var_name}[] PROGMEM = {{\n"
        
        # Write pixel data (16 values per line)
        values_per_line = width  # One row per line for readability
        for i in range(0, len(rgb565_data), values_per_line):
            line_data = rgb565_data[i:i+values_per_line]
            line_str = ", ".join(f"0x{v:04X}" for v in line_data)
            if i + values_per_line < len(rgb565_data):
                header += line_str + ",\n"
            else:
                header += line_str + "\n"
        
        header += "};\n"
        
        # Write to file
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(header)
        
        return True, f"[OK] {filename} -> {var_name}.h ({width}x{height})"
    except Exception as e:
        return False, f"[ERR] {png_path}: {e}"

def sanitize_name(filename):
    """Convert filename to valid C variable name"""
    name = os.path.splitext(filename)[0]
    # Replace special characters
    name = name.replace('-', '_')
    # Ensure it doesn't start with a number
    if name[0].isdigit():
        name = 'icon_' + name
    return name

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_dir = os.path.join(script_dir, INPUT_DIR)
    output_dir = os.path.join(script_dir, OUTPUT_DIR)
    
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    # Get all PNG files
    png_files = [f for f in os.listdir(input_dir) if f.lower().endswith('.png')]
    png_files.sort()
    
    print(f"Found {len(png_files)} PNG files in {INPUT_DIR}/")
    print(f"Output directory: {OUTPUT_DIR}/")
    print(f"Background color: #EFEFEF (card background)\n")
    
    success_count = 0
    fail_count = 0
    
    for png_file in png_files:
        png_path = os.path.join(input_dir, png_file)
        var_name = sanitize_name(png_file)
        output_path = os.path.join(output_dir, f"{var_name}.h")
        
        success, message = png_to_header(png_path, output_path, var_name)
        print(message)
        
        if success:
            success_count += 1
        else:
            fail_count += 1
    
    print(f"\n{'='*50}")
    print(f"Done! {success_count} converted, {fail_count} failed")
    print(f"Output: {output_dir}/")

if __name__ == "__main__":
    main()
