include <lib/BOSL/constants.scad>
use <lib/BOSL/shapes.scad>

module rounded_cube(size, roundness, center=false) {
    width = size[0];
    height = size[1];
    depth = size[2];

    move = center ? [-width/2, -height/2, -depth/2] : [0, 0, 0];

    translate (move)
    union() {
        for (x=[roundness, width-roundness]) {
            for (y=[roundness, height-roundness]) {
                translate([x, y, 0]) cylinder(h=depth, r=roundness);
            }
        }
        translate([roundness, 0, 0]) cube([width-roundness*2, height, depth]);
        translate([0, roundness, 0]) cube([width, height-roundness*2, depth]);
    }
}

module cube_pipe(size, wall_thickness, center=false) {
    width = size[0];
    height = size[1];
    depth = size[2];

    inner_size = [width-2*wall_thickness, height-2*wall_thickness, depth+0.2];
    inner_move = center ? [0, 0, 0] : [wall_thickness, wall_thickness, -0.1];


    difference() {
        cube([width, height, depth], center=center);
        translate(inner_move)
            cube(inner_size, center);
    }
}

module rounded_cube_pipe(size, wall_thickness, roundness, center=false) {
    width = size[0];
    height = size[1];
    depth = size[2];
    
    inner_size = [width-2*wall_thickness, height-2*wall_thickness, depth+0.2];
    inner_move = center ? [0, 0, 0] : [wall_thickness, wall_thickness, -0.1];
    inner_radius = roundness - wall_thickness;
    
    difference() {
        rounded_cube(size, roundness, center);

        if (inner_radius > 0) {
            translate(inner_move)
                rounded_cube(inner_size, inner_radius, center);
        }
        else {
            translate(inner_move)
                cube(inner_size, center);
        }
    }
}

module pipe(h, r, wall_thickness) {
    move = center ? [0, 0, 0] : [0, 0, -0.1];
    difference() {
        cylinder(h=h, r=r, center=center);
        translate(move) cylinder(h=h+0.2, r=r-wall_thickness, center=center);
    }
}

module tapered_insert_hole(h, d, taper_h, taper_deg=12) {
    taper_diam = d + 2*taper_h*tan(taper_deg);
    union() {
        translate([0, 0, -h]) cylinder(h=h, d=d);
        translate([0, 0, -taper_h/2]) cyl(h=taper_h, d1=d, d2=taper_diam);
    }
}

