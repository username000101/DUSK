pub fn input(promt: &String) -> String {
    if !promt.is_empty() {
        let str: String = dialoguer::Input::new().with_prompt(promt).interact_text().unwrap();
        str
    } else {
        let str: String = dialoguer::Input::new().interact_text().unwrap();
        str
    }
}