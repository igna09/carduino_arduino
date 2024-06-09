Import("env")

env.Replace(PROGNAME="%s" % env["PIOENV"])