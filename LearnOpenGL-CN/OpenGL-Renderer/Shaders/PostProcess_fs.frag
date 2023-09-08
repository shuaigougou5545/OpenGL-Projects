#version 330 core
out vec4 FragColor;

in vec2 uv_interp;

uniform sampler2D source;
uniform vec2 pixel_size;

#ifdef GAUSSIAN_BLUR
uniform float blur_size;
#endif


void main()
{
    vec4 color = texture(source, uv_interp);
    FragColor = color;
    
#ifdef GAUSSIAN_BLUR
    /*
     K L M
      I J
     F G H
      D E
     A B C
     */
    vec4 A = texture(source, uv_interp + pixel_size * vec2(-1.0, -1.0) * blur_size);
    vec4 B = texture(source, uv_interp + pixel_size * vec2( 0.0, -1.0) * blur_size);
    vec4 C = texture(source, uv_interp + pixel_size * vec2( 1.0, -1.0) * blur_size);
    
    vec4 D = texture(source, uv_interp + pixel_size * vec2(-0.5, -0.5) * blur_size);
    vec4 E = texture(source, uv_interp + pixel_size * vec2( 0.5, -0.5) * blur_size);
    
    vec4 F = texture(source, uv_interp + pixel_size * vec2(-1.0,  0.0) * blur_size);
    vec4 G = texture(source, uv_interp + pixel_size * vec2( 0.0,  0.0) * blur_size);
    vec4 H = texture(source, uv_interp + pixel_size * vec2( 1.0,  0.0) * blur_size);
    
    vec4 I = texture(source, uv_interp + pixel_size * vec2(-0.5,  0.5) * blur_size);
    vec4 J = texture(source, uv_interp + pixel_size * vec2( 0.5,  0.5) * blur_size);
    
    vec4 K = texture(source, uv_interp + pixel_size * vec2(-1.0,  1.0) * blur_size);
    vec4 L = texture(source, uv_interp + pixel_size * vec2( 0.0,  1.0) * blur_size);
    vec4 M = texture(source, uv_interp + pixel_size * vec2( 1.0,  1.0) * blur_size);
    
    float weight = 0.5 / 4.0; // DE IJ
    float lesser_weight = 0.125 / 4.0; // ABC FGH KLM
    
    FragColor  = (D + E + I + J) * weight;
    FragColor += (A + B + G + F) * lesser_weight;
    FragColor += (B + C + H + G) * lesser_weight;
    FragColor += (F + G + L + K) * lesser_weight;
    FragColor += (G + H + M + L) * lesser_weight;
#endif

}
