#version 120
//
////
//////
//uniform vec2 u_resolution;
//uniform vec2 u_mouse;
//uniform float u_time;
//
////  Function from Iñigo Quiles
////  www.iquilezles.org/www/articles/functions/functions.htm
//float cubicPulse( float c, float w, float x ){
//    x = abs(x - c);
//    if( x>w ) return 0.0;
//    x /= w;
//    return 1.0 - x*x*(3.0-2.0*x);
//}
//
//float plot(vec2 st, float pct){
//    return  smoothstep( pct-0.02, pct, st.y) -
//    smoothstep( pct, pct+0.02, st.y);
//}
//
//void main() {
//    vec2 st = gl_FragCoord.xy/u_resolution;
//
//    float y = cubicPulse(1.0,(st.x/st.x),1); // move line , width, direction y/x
//    vec3 color = vec3(y,1.,0.0); //change color relations
//
//    // float pct = plot(st,y);
//    // color = (1.0-pct)*color+pct*vec3(0.0,1.0,0.0);
//
//    gl_FragColor = vec4(color,1.0);
//}

//#ifdef GL_ES
//precision mediump float;
//#endif
//
//uniform vec2 u_resolution;
//uniform float u_time;

// Author @patriciogv - 2015
// Title: Mosaic

//#ifdef GL_ES
//precision mediump float;
//#endif
//
//uniform vec2 u_resolution;
//uniform vec2 u_mouse;
//uniform float u_time;
//
//float random (vec2 st) {
//    return fract(sin(dot(st.xy,
//                         vec2(-0.340,-0.100)))*
//                 u_time);
//}
//
//void main() {
//    vec2 st = gl_FragCoord.xy/u_resolution.xy;
//
//    st *= 10.0; // Scale the coordinate system by 10
//    vec2 ipos = floor(st);  // get the integer coords
//    vec2 fpos = fract(st);  // get the fractional coords
//
//    // Assign a random value based on the integer coord
//    vec3 color = vec3(random( ipos ));
//
//    // Uncomment to see the subdivided grid
//    // color = vec3(fpos,0.0);
//
//    gl_FragColor = vec4(color,1.0);
//}



#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(-0.980,0.830)))*
                 u_time);
}

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    
    float rnd = random( st );
    
    gl_FragColor = vec4(vec3(rnd),1.0);
}
