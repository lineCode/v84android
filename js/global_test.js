/**
 * Created by guangongbo on 2017/3/3.
 */
"use strict";

function dump(s, tab, name, obj) {
    for (let i = 0; i < tab; ++i) {
        s += "  ";
    }
    s += name;
    s += " : ";
    if (typeof obj == "object") {
        s += "{";
        let has = false;
        for (let e in obj) {
            if (!has) {
                has = true;
                s += "\n";
            }
            s = dump(s, tab + 1, e, obj[e]);
        }
        for (let i = 0; i < tab; ++i) {
            s += "  ";
        }
        s += "}";
    } else {
        s += typeof obj;

        if (typeof obj == "boolean" || typeof obj == "number" || typeof obj == "string") {
            s += ' "';
            s += obj;
            s += '"';
        }
    }
    s += ", \n";

    return s;
}

let s = dump("", 0, "this", this);
log(s);