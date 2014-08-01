_mhng_scan() {
    local cur
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"

    if [[ ${COMP_CWORD} -eq 1 ]]
    then
        COMPREPLY=( $(compgen -W "$(mhng-pipe-folder_names)" -- ${cur}) )
    fi
}
complete -F _mhng_scan mhng-scan
complete -F _mhng_scan scan

_mhng_show() {
    local cur prev
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    if [[ ${COMP_CWORD} -eq 1 ]]
    then
        COMPREPLY=( $(compgen -W "$(mhng-pipe-sequence_numbers)" -- ${cur}) )
    else
        COMPREPLY=( $(compgen -W "$(mhng-pipe-sequence_numbers ${prev})" -- ${cur}) )
    fi
}
complete -F _mhng_show mhng-show
complete -F _mhng_show show
