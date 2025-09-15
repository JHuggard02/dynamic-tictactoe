
export default function Block({clicked, clickFn, row, col, shape, width, height, rows, cols}) {
    const borderText = "1px solid grey";
    let borderStyle={};
    if (row == 0 && col == 0){
        borderStyle={borderBottom: borderText, borderRight: borderText};
    }else if (row == 0 && col == cols - 1){
        borderStyle={borderBottom: borderText, borderLeft: borderText};
    }else if (row == 0){
        borderStyle={borderLeft: borderText, borderRight: borderText, borderBottom: borderText};
    }else if (row == rows - 1 && col == 0){
        borderStyle={borderTop: borderText, borderRight: borderText};
    }else if (row == rows -1 && cols == cols - 1){
        borderStyle={borderLeft: borderText, borderTop: borderText};
    }else if (row == rows - 1){
        borderStyle={borderTop: borderText, borderLeft: borderText, borderRight: borderText};
    }else if (col == 0){
        borderStyle={borderTop: borderText, borderRight: borderText, borderBottom: borderText};
    }else if (col == cols - 1){
        borderStyle={borderTop: borderText, borderLeft: borderText, borderBottom: borderText};
    }else{
        borderStyle={border: borderText};
    }
    return(
        <div style={{width: width, height: height, padding: "5px", ...borderStyle}}>
            {!clicked ?
            <div style={{width: "100%", height: "100%", cursor: "pointer"}}
                onClick={() => clickFn(row, col)}>   
            </div>
            :
            <div style={{width: "100%", height: "100%", display: "flex", alignItems: "center", justifyContent: "center", color:"black"}}
                >
                {shape}
            </div>
            }
        </div>
        
    )
}