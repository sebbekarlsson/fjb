(function () {
  const DONT_BE_HERE = 33;
  const thisShouldNotBeHere = function() {
    console.log(1);
  };
  function shouldNotBeHere()
  {

  }
  const obj = {
    x: 2,
    y: 4
  };
  const run = (function() {
    return z;
  });

  const _ = run();

  const doit = overArg(obj.y, 4);


  function z() {
    if (3 >= 3)
    {
      console.log(x);
    }
    return doit();
  }

  /**
   * Creates a unary function that invokes `func` with its argument transformed.
   *
   * @private
   * @param {Function} func The function to wrap.
   * @param {Function} transform The argument transform.
   * @returns {Function} Returns the new function.
   */
  function overArg(func, transform) {
    console.log(func);
    console.log(transform);
    return function(arg) {
      console.log(arg);
      return doot(); 
    };
  }

  function doot() {
    console.log(1);
    return 1;
  } 

  /** Detect free variable `exports`. */
  var freeExports = typeof exports == 'object' && exports && !exports.nodeType && exports;

  /** Detect free variable `module`. */
  var freeModule = freeExports && typeof module == 'object' && module && !module.nodeType && module;

/*  if (freeModule)
  {
    freeModule.exports.z = _;
  }*/

  module.exports.z = _;
}.call(this));
