pcb_width = 28.956;
pcb_height = 36.576;
pcb_depth = 1.6;

pcb_usb_width = 2.9;
pcb_usb_height = 8;
pcb_usb_depth = 7.1;

pcb_usb_x = 2.286;
pcb_usb_y = 9.144;

pcb_screw_name = "M2";
pcb_screw_diam = 2;
pcb_screw_offset = 2.286;

module pcb() {
    width = 28.956;
    height = 36.576;
    
    color ("purple") {
        translate([width/2, height/2, 1.6]) { 
            import("res/paperClockPCB.stl", convexity=10);
        }
    }

    usb_width = 2.9;
    usb_height = 8;
    usb_depth = 7.1;
    
    usb_center_x = 2.286;
    usb_center_y = 9.144;

    translate([usb_center_x-usb_width/2, usb_center_y-usb_height/2, -usb_depth])
        color("gray") cube([usb_width, usb_height, usb_depth]);
}
