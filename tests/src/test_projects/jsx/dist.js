/* IMPORTED FROM `index.jsx` */
function render(firstname, lastname){return ((function(){ let parent = document.createElement("div");parent.setAttribute("title", "hello");

 
parent.setAttribute("data-name", `${lastname}`);

 
;parent.appendChild((function(){ let parent = document.createElement("h1");;parent.appendChild(document.createTextNode(`Welcome ${firstname}`))
;return parent;})()


)
parent.appendChild((function(){ let parent = document.createElement("p");;parent.appendChild(document.createTextNode(`Hello ${firstname} ${lastname}
      `))
;return parent;})()


)
;return parent;})()


);};document . addEventListener('DOMContentLoaded', function _0x55d09936c1d0(){document . getElementById("root") . appendChild(render("John", "Doe"));});
