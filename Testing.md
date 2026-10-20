## Test-Environment
For testing deletion of logs, optionally copy any existing logs into a test folder, then run eg
`(for /l %i in (10,1,30) do @touch access.log.2025.06.%i) & (for /l %i in (10,1,31) do @touch access.log.2025.07.%i)`

## Sample Config File
```ini
[default]
log_path = c:\wampAideTest
minimum_logs = 14
```

## Some Suggested Tests
- Version info:		`wampAide -v` (or `-V`, `--version`)
- Help menu:		`wampAide -h` (or `--help`)
- No parameters:	`wampAide`
- Missing config:	`wampAide -c`
- Incorrect config:	`wampAide -c test.in`
- Only config:		`wampAide -c test.ini`
- Less than config:	`wampAide -c test.ini -l10`
- Expect deletion:	`wampAide -c test.ini -l40`

### Config File

- During development I did test a few varieties of the config file, eg a non-existent path, but I think that if ever I do further work on wampAide, I should make it scan the whole config file, and report all errors before quitting. Then I should create a test file with a variety of errors.
