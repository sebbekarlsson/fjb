/* Parsing...*/
/* Visiting...*/
/* Parsing...*/
/* Visiting...*/
/* Generating...*/
/*

=======================
OBJECT()
*/
/* Generating...*/
/*

=======================
OBJECT()
*/
/* IMPORTED FROM `src/index.js` */
var tmp = (function(){
 let exports = {}; let module = { 'exports': exports };
/* IMPORTED FROM `node_modules/lodash/lodash.js` */
function isSymbol(value){return typeof value == "symbol" || (isObjectLike(value) && baseGetTag(value) == symbolTag);};function isObject(value){var type=typeof value;return value != null && (type == "object" || type == "function");};function toNumber(value){if(typeof value == "number"){return value;};if(isSymbol(value)){return NAN;};if(isObject(value)){var other=typeof value . valueOf == "function"?value . valueOf():value;value=isObject(other)?(other + ""):other;};if(typeof value != "string"){return value === 0?value:+value;};value=value . replace(reTrim, "");var isBinary=reIsBinary . test(value);return (isBinary || reIsOctal . test(value))?freeParseInt(value . slice(2), isBinary?2:8):(reIsBadHex . test(value)?NAN:+value);};function toString(value){return value == null?"":baseToString(value);};function createRound(methodName){var func=Math[methodName];return function (number, precision){number=toNumber(number);precision=precision == null?0:nativeMin(toInteger(precision), 292);if(precision && nativeIsFinite(number)){var pair=(toString(number) + "e") . split("e"), value=func(pair[0] + "e" + (+pair[1] + precision));pair=(toString(value) + "e") . split("e");return +(pair[0] + "e" + (+pair[1] - precision));};return func(number);};};var ceil=createRound("ceil");module.exports.ceil = ceil;

return module && module.exports ? module.exports : exports;
})()
;let ceil = tmp.ceil
console . log(ceil(24.5));
