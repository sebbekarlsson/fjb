(function(){
  function add(x, y) {
    return x + y;
  }

  function subtract(x, y) {
    return x - y;
  }

  function mul(x, y) {
    return x * y;
  }

  module.exports.add = add;
  module.exports.subtract = subtract;
  module.exports.mul = mul;
}).call(this);
