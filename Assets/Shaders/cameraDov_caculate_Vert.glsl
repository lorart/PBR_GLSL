uniform mat4 ProjectMatrix;
uniform mat4 ModelViewMatrix;
 
attribute vec4 a_position;
attribute vec2 a_texcoord;
 
varying vec2 v_texcoord;
varying float v_depth;
 
void main()
{
    gl_Position = ModelViewMatrix * a_position; // 转换到视点空间
 
    v_texcoord = a_texcoord;
    v_depth = gl_Position.z; // 记录一下相机深度信息
 
    gl_Position = ProjectMatrix * gl_Position; // 转换到投影空间
}
    