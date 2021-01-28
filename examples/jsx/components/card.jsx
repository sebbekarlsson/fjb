const cardStyle = `
background-color: white;
color: black;
box-shadow: 0 3px 6px rgba(0,0,0,0.16), 0 3px 6px rgba(0,0,0,0.23);
width: 200px;
height: 200px;
display: grid;
grid-template-rows: 20% auto;
font-family: Sans-Serif;
cursor: pointer;
`;

const cardHeaderStyle = `
  display: flex;
  justify-content: center;
  align-items: center;
  color: white;
  background-color: rgb(48, 48, 48);
`;

const cardBodyStyle = `
  box-sizing: border-box;
  padding: 1rem;
`;

export function CardHeader() {
  return <section style={cardHeaderStyle}></section>;
}

export function CardBody() {
  return <section style={cardBodyStyle}></section>;
}

export function Card(name, description) {
  this.state = { toggled: false };

  this.toggle = function () {
    this.state.toggled = !this.state.toggled;
  };

  handleClick = function (e) {
    e.preventDefault();
    this.toggle();
    this.ref.style.backgroundColor = this.state.toggled ? "purple" : "";
  }.bind(this);

  return (
    <div onclick={handleClick} style={cardStyle} title={name}>
      <CardHeader>{name}</CardHeader>
      <CardBody>{description}</CardBody>
    </div>
  );
}
