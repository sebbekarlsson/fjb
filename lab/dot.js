/* Parsing...*/
/* Visiting...*/
/* Parsing...*/
/* Visiting...*/
/* Generating...*/
/* Generating...*/
/* IMPORTED FROM `src/index.js` */
var tmp = (function(){

if (typeof exports === "undefined")
{
  let exports = {};
}

if (typeof module === "undefined")
{
  let module = { 'exports': exports };
}


/* IMPORTED FROM `node_modules/lodash/lodash.js` */
function isObject(value){var type=typeof value;return value != null && (type == "object" || type == "function");}
this.isObject= isObject;
;function isSymbol(value){return typeof value == "symbol" || (isObjectLike(value) && baseGetTag(value) == symbolTag);}
this.isSymbol= isSymbol;
;function baseToString(value){if(typeof value == "string"){return value;};if(isArray(value)){return arrayMap(value, baseToString) + "";};if(isSymbol(value)){return symbolToString?symbolToString . call(value):"";};var result=(value + "");return (result == "0" && (1 / value) == -INFINITY)?"-0":result;}
this.baseToString= baseToString;
;function toFinite(value){if(!value){return value === 0?value:0;};value=toNumber(value);if(value === INFINITY || value === -INFINITY){var sign=(value < 0?-1:1);return sign * MAX_INTEGER;};return value === value?value:0;}
this.toFinite= toFinite;
;function result(object, path, defaultValue){path=castPath(path, object);var index=-1, length=path . length;if(!length){length=1;object=undefined;};while(++index < length){var value=object == null?undefined:object[toKey(path[index])];if(value === undefined){index=length;value=defaultValue;};object=isFunction(value)?value . call(object):value;};return object;}
this.result= result;
;function toString(value){return value == null?"":baseToString(value);}
this.toString= toString;
;function toInteger(value){var result=toFinite(value), remainder=result % 1;return result === result?(remainder?result - remainder:result):0;}
this.toInteger= toInteger;
;function toNumber(value){if(typeof value == "number"){return value;};if(isSymbol(value)){return NAN;};if(isObject(value)){var other=typeof value . valueOf == "function"?value . valueOf():value;value=isObject(other)?(other + ""):other;};if(typeof value != "string"){return value === 0?value:+value;};value=value . replace(reTrim, "");var isBinary=reIsBinary . test(value);return (isBinary || reIsOctal . test(value))?freeParseInt(value . slice(2), isBinary?2:8):(reIsBadHex . test(value)?NAN:+value);}
this.toNumber= toNumber;
;function createRound(methodName){var func=Math[methodName];return function (number, precision){number=toNumber(number);precision=precision == null?0:nativeMin(toInteger(precision), 292);if(precision && nativeIsFinite(number)){var pair=(toString(number) + "e") . split("e"), value=func(pair[0] + "e" + (+pair[1] + precision));pair=(toString(value) + "e") . split("e");return +(pair[0] + "e" + (+pair[1] - precision));};return func(number);};}
this.createRound= createRound;
;var ceil=createRound("ceil")
this.ceil= ceil;

return this;
})()
;let ceil = tmp.ceil
console . log(ceil(24.5));
