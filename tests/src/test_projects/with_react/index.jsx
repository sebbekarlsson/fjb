import React, { Component } from "react";
import { render } from "react-dom";


class App {
  render() {
    return (
      <div>
        <h2>Welcome</h2>
      </div>
    );
  }
}

render(<App />, document.getElementById("root"));
