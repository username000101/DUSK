use tracing_appender::non_blocking;

pub fn init_log() -> non_blocking::WorkerGuard {
    let (non_blocking, guard) = non_blocking(std::io::stdout());
    
    tracing_subscriber::fmt()
        .with_timer(tracing_subscriber::fmt::time::LocalTime::rfc_3339())
        .with_writer(non_blocking)
        .with_max_level(if cfg!(debug_assertions) { tracing::Level::DEBUG } else { tracing::Level::INFO })
        .pretty()
        .init();
    guard
}