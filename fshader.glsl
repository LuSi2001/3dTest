varying vec3 v_coord;

void main()
{
    vec4 color;
    float v = v_coord.z/30;
    if(v_coord.z < 0.0) color = vec4(0, 0, 1 + v, 1);
    else if(v_coord.z < 0.2) color = vec4(0.8 - v * 6, 0.8 - v * 6, 0.1 - v, 1);
    else if(v_coord.z < 1) color = vec4(0.2 + v, 0.2 + v, 0.01 + v, 1);
    else if(v_coord.z < 3) color = vec4(0, 0.4 + v, 0, 1);
    else if(v_coord.z < 8.0) color = vec4(0.4 + v, 0.4 + v, 0.4 + v, 1);
    else color = vec4(0.65 + v, 0.65 + v, 0.65 + v, 1);
    gl_FragColor = color;
}
