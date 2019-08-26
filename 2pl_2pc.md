# Bug
- Transactions that have committed are not releasing their locks.
- After looking at terminal output: The unreleased lock is from a transaction that originated on the other thread?
    - Seems to be true (confirmed for 4 txns)
    - The only txns that aren't releasing locks are those that used the 1pc shortcut
