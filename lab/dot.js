/* IMPORTED FROM `src/simple.js` */
var _122 = (function(){

/* IMPORTED FROM `src/./helpers.js` */
function doot(){console . log(1);return 1;}
this.doot= doot;
;function overArg(func, transform){console . log(func);console . log(transform);return function (arg){console . log(arg);return doot();};}
this.overArg= overArg;
;const obj={x :2, y :4};const doit=overArg(obj . y, 4);function z(){if(3 >= 3){console . log("it works");};return doit();}
this.z= z;

return this;
})()
;let z = _122.z
z();
