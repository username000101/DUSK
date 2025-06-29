use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize)]
pub struct Module {
    pub id: String,
    pub name: Option<String>,
    pub author: Option<String>,
    pub version: Option<String>,
    pub description: Option<String>,
    pub entry_points: Vec<(String, String)>
}