const style = {
  'width': '250px',
  'height': '250px',
  'backgroundColor': 'white',
  'borderStyle': 'solid',
  'borderWidth': '1px',
  'borderColor': 'lightgray',
  'boxSizing': 'border-box',
  'padding': '1rem',
  'display': 'flex',
  'justifyContent': 'center',
  'alignItems': 'center'
}

export const Card = (props) => {
  const handleClick = (e) => console.log(e, 'clicked');

  return <div onClick={handleClick} style={style}>
    <p>{ props.text }</p>
  </div>
};
