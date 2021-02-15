const Vue = require('vue/dist/vue.js');

var app = new Vue({
  el: '#root',
  data: {
    message: "hello"
  }
  /*render(h)
  {
    return h('div', "Hello")
  }*/
});

console.log(app);
