$fa = 0.2;
$fs = 0.2;

display_screw_name = "M3";
display_screw_diam = 3;
display_screw_dist = 2.5;

display_width = 89.50;
display_height = 38.00;
display_depth = 2.9;

display_pcb_depth = 1.65;

display_screen_start_x = 8.5;
display_screen_end_x = display_width - 14;
display_screen_start_y = 4.5;
display_screen_end_y = display_height - 4.5;
display_screen_width = display_screen_end_x - display_screen_start_x;
display_screen_height = display_screen_end_y - display_screen_start_y;

/*
    Screw size: M3 or M2.8 (try M3 first - longer lengths)
*/
module display() {
    // PCB
    width = display_width;
    height = display_height;
    depth = display_pcb_depth;
    
    // Screws
    radius = display_screw_diam/2;
    dist = display_screw_dist;

    // "Paper"
    paper_depth = display_depth - depth;
    paper_start_x = 5;
    paper_end_x = width - 5;
    paper_width = paper_end_x - paper_start_x;
    paper_height = height;

    // Display
    display_start_x = display_screen_start_x;
    display_end_x = display_screen_end_x;
    display_start_y = display_screen_start_y;
    display_end_y = display_screen_end_y;
    display_width = display_screen_width;
    display_height = display_screen_height;

    // Connector
    conn_start_x = -2;
    conn_end_x = 7.5;
    conn_start_y = 9;
    conn_end_y = height - 9;
    conn_width = conn_end_x - conn_start_x;
    conn_height = conn_end_y - conn_start_y;
    conn_depth = 5.5;
    
    // Ribbon Connector
    rib_start_x = width - 10;
    rib_end_x = width;
    rib_start_y = 12;
    rib_end_y = height - 11;
    rib_width = rib_end_x - rib_start_x;
    rib_height = rib_end_y - rib_start_y;
    rib_depth = 2;

    // PCB
    difference() {
        color("blue") cube([width, height, depth]);
        // Screws
        for (x=[0+dist, width-dist]) {
            for (y=[0+dist, height-dist]) {
                translate([x, y, -0.1]) cylinder(h=depth+0.2, r=radius);
            }
        }
    }

    // "Paper"
    color("gray")
        translate([paper_start_x, 0, depth])
            cube([paper_width, paper_height, paper_depth]);

    // Display
    color("white")
        translate([display_start_x, display_start_y, depth+paper_depth])
            cube([display_width, display_height, 0.1]);

    // Connector
    color("tan")
        translate([conn_start_x, conn_start_y, -conn_depth])
            cube([conn_width, conn_height, conn_depth]);

    color("tan")
        translate([rib_start_x, rib_start_y, -rib_depth])
            cube([rib_width, rib_height, rib_depth]);
}

