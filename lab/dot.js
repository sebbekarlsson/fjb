const apa= (function(){
 let exports = {}; let module = { 'exports': exports };
const apa=()=>console . log("apa apansson");
module . exports=apa;

/* noop */


return module && module.exports ? module.exports : exports;
})()
;
const getItems=()=>[1,2,3];
getItems()[0];

/* noop */


