uniform sampler2D texture;
varying float v_depth;
varying vec2 v_texcoord;
 
void main(void)
{
    gl_FragColor = texture2D(texture, v_texcoord);
 
    float blur = 0;
 
    float near_distance = 10.0; // 近平面的模糊衰减范围
    float far_distance = 10.0; // 远平面的模糊衰减范围
 
    float near_plane = -20.0; // 近平面
    float far_plane = -25.0; // 远平面
 
    // 根据深度计算模糊因子
    if(v_depth <= near_plane && v_depth >= far_plane)
    {
        blur = 0;
    }
    else if(v_depth > near_plane)
    {
        blur = clamp(v_depth, near_plane, near_plane + near_distance);
        blur = (blur - near_plane) / near_distance;
    }
    else if(v_depth < far_plane)
    {
        blur = clamp(v_depth, far_plane - far_distance, far_plane);
        blur = (far_plane - blur) / far_distance;
    }
 
    // 将模糊因子写入alpha通道
    gl_FragColor.a = blur;
}
    