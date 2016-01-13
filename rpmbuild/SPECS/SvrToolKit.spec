Name:		SvrToolKit
Version:	1.1
Release:	1%{?dist}
Summary:	Service Tool Kit

Group:		Development/Tools
License:	Commercial
URL:		http://portal.titan.imgo.tv/
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

#BuildRequires:	
#Requires:	


%description


%prep
%setup -q


%build


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/bin/
cp SvrScheduler.sh  %{buildroot}/usr/bin/
cp LogTruncate.sh %{buildroot}/usr/bin/

%post
echo "%{name}-%{version} installed into /usr/bin"


%postun
echo "%{name}-%{version} uninstalled from /usr/bin"


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc
/usr/bin/SvrScheduler.sh
/usr/bin/LogTruncate.sh


%changelog

