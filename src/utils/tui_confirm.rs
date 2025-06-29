pub fn confirm(promt: &String) -> bool {
    if !promt.is_empty() {
        let is : bool =  dialoguer::Confirm::new().with_prompt(promt).interact().unwrap();
        is
    } else {
        let is : bool = dialoguer::Confirm::new().interact().unwrap();
        is
    }
}