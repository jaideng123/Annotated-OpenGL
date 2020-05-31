#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  

vec4 apply_kernel(float[9] kernel);

void main()
{ 
    // Normal
    FragColor = texture(screenTexture, TexCoords);

    // Invert
    // FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

    // Greyscale (w/ weighted channels for more accurate result)
    // FragColor = texture(screenTexture, TexCoords);
    // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    // FragColor = vec4(average, average, average, 1.0);

    // Sharpen Filter (Uses Kernel + grabs surrounding color values)
    // float kernel[9] = float[](
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // );
    // FragColor = apply_kernel(kernel);

    // Blur Filter
    // float kernel[9] = float[](
    // 1.0 / 16, 2.0 / 16, 1.0 / 16,
    // 2.0 / 16, 4.0 / 16, 2.0 / 16,
    // 1.0 / 16, 2.0 / 16, 1.0 / 16  
    // );
    // FragColor = apply_kernel(kernel);

    // Edge Detection
    // float kernel[9] = float[](
    // 1, 1, 1,
    // 1,-8, 1,
    // 1, 1, 1  
    // );
    // FragColor = apply_kernel(kernel);
  
}

vec4 apply_kernel(float[9] kernel){
    // Kernel = matrix centered on current pixel, used to sum together surrounding pixels
    // Most should sum to 1
      vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );
    
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return vec4(col, 1.0);
}