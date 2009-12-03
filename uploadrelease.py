# -*- coding: utf-8 -*-
import sys, os, launchpadlib, subprocess, codecs
from launchpadlib.launchpad import Launchpad, STAGING_SERVICE_ROOT, EDGE_SERVICE_ROOT
from launchpadlib.credentials import Credentials

#serviceRoot = STAGING_SERVICE_ROOT
serviceRoot = EDGE_SERVICE_ROOT
projectName = 'tagaini-jisho'
cachedir = "/tmp/tagainiuploader/cache/"
credentialsfile = "credentials.txt"
releaseVersion = "0.2.396"
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
			mac='Intel MacOS X binary (%s version)')

FILE_CONTENTTYPES = dict(source='application/x-tar',
			win32='application/exe',
			mac='application/x-apple-diskimage')

LANGUAGES= ('English', 'French', 'German', 'Spanish', 'Russian')
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
	releaseFile = release.add_file(filename = f, description = finalDesc, file_content = codecs.open(f, 'rb', 'latin').read(), content_type = FILE_CONTENTTYPES[fType], file_type = FILE_TYPES[fType], signature_filename = fSign, signature_content = codecs.open(fSign, 'rb', 'latin').read())
	release.lp_save()

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
	if release.version == releaseVersion:
		try:
			sys.stdout.write("Found the release. I will now create signature files - please enter your GPG private key passphrase: ")
			gpgPass = sys.stdin.readline()
			# Upload the source tarball
			#uploadFile('tagainijisho-' + releaseVersion + '.tar.gz', 'source', '', gpgPass)
			# Upload the win32 binaries
			for lang in LANGUAGES:
				uploadFile('tagainijisho-' + releaseVersion + '-' + LANGUAGES_SUFFIXES[lang] + '.exe', 'win32', lang, gpgPass)
			# Upload the mac binaries
			#for lang in LANGUAGES:
			#	uploadFile('Tagaini Jisho-' + releaseVersion + '-' + LANGUAGES_SUFFIXES[lang] + '.dmg', 'mac', lang, gpgPass)
		except launchpadlib.errors.HTTPError, e:
			print "An error happened while uploading."
			print e.content
			releaseFile = None
		sys.exit(0)
print "Release not found - please create it on Launchpad first."

