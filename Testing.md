## Test-Environment
For testing deletion of logs, optionally copy any existing logs into a test folder, then run eg
`(for /l %i in (10,1,30) do @touch access.log.2025.06.%i) & (for /l %i in (10,1,31) do @touch access.log.2025.07.%i)`

## Sample Config File
    > [default]
    > log_path = c:\wampAideTest
    > minimum_logs = 14

## Some Suggested Tests
- No parms:		`cargo run`
- Missing config:	`cargo run -- -c`
- Incorrect config:	`cargo run -- -c test.in`
- Only config:		`cargo run -- -c test.ini`
- Less than config:	`cargo run -- -c test.ini -l10`
- Expect deletion:	`cargo run -- -c test.ini -l40`

### Config File

- During development I did make a few changes to the config file, eg non-existent path, but I think that if ever I do further work on wampAide, I should make it scan the whole config file, and report all errors before quitting. Then I should provide a test file with a variety of errors.
