uniform mat4 mvp_matrix;

attribute vec4 a_position;
attribute vec3 a_coord;

varying vec3 v_coord;

void main()
{
    gl_Position = mvp_matrix * a_position;

    v_coord = a_coord;
}
