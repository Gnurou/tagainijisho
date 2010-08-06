# -*- coding: utf-8 -*-
import sys, os, launchpadlib, subprocess, codecs, re, os.path, getpass
from launchpadlib.launchpad import Launchpad, STAGING_SERVICE_ROOT, EDGE_SERVICE_ROOT, LPNET_SERVICE_ROOT
from launchpadlib.credentials import Credentials
import lazr.restfulclient.errors

#serviceRoot = STAGING_SERVICE_ROOT
#serviceRoot = EDGE_SERVICE_ROOT
serviceRoot = LPNET_SERVICE_ROOT
projectName = 'tagaini-jisho'
cachedir = "/tmp/tagainiuploader/cache/"
credentialsfile = "credentials.txt"
releaseVersion = None
# Get the current release
for line in open("CMakeLists.txt"):
	if "set(VERSION " in line:
		releaseVersion = re.match("set\\(VERSION (.*)\\)", line).group(1)
		break
if not releaseVersion:
	print "Cannot find release version"
	sys.exit(1)
# Build the launchpad release name
vers = releaseVersion.split('.')
if len(vers) == 4: vers[2] = str(int(vers[2]) + 1)
lpRelease = '.'.join(vers[0:3])
if len(vers) == 4:
	if vers[3] == "80": lpRelease += "b1"
	elif vers[3] == "85": lpRelease += "b2"
	elif vers[3] == "90": lpRelease += "rc1"
	elif vers[3] == "95": lpRelease += "rc2"

launchpad = None

FILE_TYPES = dict(source='Code Release Tarball',
			win32='Installer file',
			mac='Installer file',
			readme='README File',
			release_notes='Release Notes',
			changelog='ChangeLog File',
			installer='Installer file')

FILE_DESCRIPTIONS = dict(source='Source release',
			win32='Win32 binary (%s version)',
			mac='Mac OS X universal binary (%s version)')

FILE_CONTENTTYPES = dict(source='application/x-tar',
			win32='application/exe',
			mac='application/x-apple-diskimage')

LANGUAGES= ('Russian', 'Spanish', 'German', 'French', 'English')
LANGUAGES_SUFFIXES = dict(English='en', French='fr', German='de', Spanish='es', Russian='ru')

def manualAuthentication():
	global launchpad
	launchpad = Launchpad.get_token_and_login('Tagaini Jisho release uploader', serviceRoot, cachedir)
	launchpad.credentials.save(file(credentialsfile, 'w'))

def signFile(f, gpgPass):
	subprocess.Popen(args = ["gpg", "--armor", "--sign", "--detach-sig", "--batch", "--passphrase-fd", "%d" % (0), f], stdin = subprocess.PIPE).communicate(gpgPass)
	
def uploadFile(f, fType, lang, gpgPass):
	if not os.path.exists(f):
		print "Cannot find file %s, skipping..." % (f)
	global FILE_DESCRIPTIONS, FILE_CONTENTTYPES, FILE_TYPES
	fSign = f + ".asc"
	signFile(f, gpgPass)
	print "Uploading", f
	finalDesc = FILE_DESCRIPTIONS[fType]
	if lang: finalDesc = finalDesc % (lang)
	try:
		releaseFile = release.add_file(filename = os.path.basename(f), description = finalDesc, file_content = open(f, 'r').read(), content_type = FILE_CONTENTTYPES[fType], file_type = FILE_TYPES[fType], signature_filename = fSign, signature_content = open(fSign, 'r').read())
	# Don't know why, but an exception will always be thrown even though the file is uploaded
	except lazr.restfulclient.errors.HTTPError:
		pass

for arg in sys.argv:
	if arg == "--auth":
		manualAuthentication()

if not launchpad:
	if os.path.exists(credentialsfile):
		credentials = Credentials()
		credentials.load(open(credentialsfile))
		launchpad = Launchpad(credentials, serviceRoot, cachedir)
	else:
		manualAuthentication()

project = launchpad.projects[projectName]
for release in project.releases:
	if release.version == lpRelease:
		gpgPass = getpass.getpass("Found the release. I will now create signature files - please enter your GPG private key passphrase: ")
		# Upload the source tarball
		uploadFile('pack/source/tagainijisho-' + releaseVersion + '.tar.gz', 'source', '', gpgPass)
		# Upload the mac binaries
		for lang in LANGUAGES:
			uploadFile('pack/MacOS/Tagaini Jisho-' + releaseVersion + '-' + LANGUAGES_SUFFIXES[lang] + '.dmg', 'mac', lang, gpgPass)
		# Upload the win32 binaries
		for lang in LANGUAGES:
			uploadFile('pack/win32-cross/tagainijisho-' + releaseVersion + '-' + LANGUAGES_SUFFIXES[lang] + '.exe', 'win32', lang, gpgPass)
		release.lp_save()
		sys.exit(0)
print "Release not found - please create it on Launchpad first."

