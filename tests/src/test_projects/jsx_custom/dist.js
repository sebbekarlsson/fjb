HeadTitle
Text
/* IMPORTED FROM `index.jsx` */
function HeadTitle(title){return (function(){ let parent = document.createElement("h1");;parent.appendChild(document.createTextNode(`${title}`))
;return parent;})()


;}
try {this.HeadTitle= HeadTitle;
} catch(e){};function Text(text){return (function(){ let parent = document.createElement("p");;parent.appendChild(document.createTextNode(`${text}`))
;return parent;})()


;}
try {this.Text= Text;
} catch(e){};function Section(title, text){return ((function(){ let parent = document.createElement("section");;parent.appendChild((function(){ let parent = HeadTitle(title=`${title}`); HeadTitle;parent.setAttribute("title", `${title}`);

 
;return parent;})()


)
parent.appendChild((function(){ let parent = Text(text=`${text}`); Text;parent.setAttribute("text", `${text}`);

 
;return parent;})()


)
;return parent;})()


);}
try {this.Section= Section;
} catch(e){};function App(){return Section("Hello", "welcome to my website");}
try {this.App= App;
} catch(e){};document . getElementById("root") . appendChild(App());
