# .github/ISSUE_TEMPLATE/feature_request.md

---
name: Feature Request
about: Suggest an idea for this project
title: '[FEATURE] '
labels: enhancement
assignees: ''
---

## User Story
**As a** [type of user]  
**I want** [functionality]  
**So that** [benefit/value]

## Problem Statement
Describe the problem this feature would solve or the opportunity it addresses.

## Proposed Solution
Describe your preferred solution and how it should work.

## Acceptance Criteria
- [ ] Criterion 1 (specific, measurable outcome)
- [ ] Criterion 2 (specific, measurable outcome)
- [ ] Criterion 3 (specific, measurable outcome)

## Technical Requirements

### API Changes
- [ ] New endpoints needed: 
- [ ] Existing endpoints modified:
- [ ] Authentication/authorization changes:

### Database Changes
- [ ] New tables/columns:
- [ ] Schema modifications:
- [ ] Data migration required:

### Infrastructure Changes
- [ ] Docker configuration updates:
- [ ] AWS resources needed:
- [ ] Environment variables:

## Implementation Notes
- **Estimated Effort**: [Small/Medium/Large]
- **Priority**: [High/Medium/Low]
- **Dependencies**: List any dependent issues or external requirements

## Testing Strategy
- [ ] Unit tests for new functionality
- [ ] Integration tests for API endpoints
- [ ] Performance tests (if applicable)
- [ ] Manual testing scenarios

## Definition of Done
- [ ] Code implemented and reviewed
- [ ] Tests written and passing
- [ ] Documentation updated
- [ ] Deployed to staging environment
- [ ] User acceptance testing completed

---

# .github/ISSUE_TEMPLATE/bug_report.md

---
name: Bug Report
about: Create a report to help us improve
title: '[BUG] '
labels: bug
assignees: ''
---

## Bug Description
**Summary**: Brief description of the issue

**Environment**:
- **Version**: [e.g., v1.0.0]
- **Environment**: [development/staging/production]
- **Browser**: [if applicable]
- **OS**: [if applicable]

## Steps to Reproduce
1. Step 1
2. Step 2
3. Step 3
4. See error

## Expected Behavior
Describe what should have happened.

## Actual Behavior
Describe what actually happened.

## Screenshots/Logs
If applicable, add screenshots or log excerpts to help explain the problem.

```
Paste any relevant log output here
```

## Additional Context
- **API Endpoint**: [if applicable]
- **Request Payload**: [if applicable]
- **Database State**: [if relevant]
- **Related Issues**: #123, #456

## Impact Assessment
- **Severity**: [Critical/High/Medium/Low]
- **Users Affected**: [All/Specific group/Individual]
- **Workaround Available**: [Yes/No - describe if yes]

## Debugging Information
- **Error Codes**: 
- **Stack Trace**: 
- **Database Query**: [if applicable]
- **Container Logs**: [if applicable]

---

# .github/ISSUE_TEMPLATE/technical_debt.md

---
name: Technical Debt
about: Address technical debt and code improvements
title: '[TECH-DEBT] '
labels: technical-debt
assignees: ''
---

## Technical Debt Description
**Issue**: Brief description of the technical debt

**Location**: 
- **Files/Modules Affected**: 
- **Code Lines**: [if specific]

## Current Problems
- [ ] Performance issues
- [ ] Maintainability concerns
- [ ] Security vulnerabilities
- [ ] Code duplication
- [ ] Outdated dependencies
- [ ] Missing tests
- [ ] Poor documentation

## Proposed Solution
Describe the refactoring or improvement approach.

## Benefits
- **Performance**: Expected improvements
- **Maintainability**: How it makes code easier to work with
- **Security**: Any security improvements
- **Developer Experience**: How it helps the development team

## Implementation Plan
1. **Phase 1**: 
2. **Phase 2**: 
3. **Phase 3**: 

## Risk Assessment
- **Breaking Changes**: [Yes/No - describe if yes]
- **Deployment Risk**: [Low/Medium/High]
- **Testing Requirements**: 
- **Rollback Strategy**: 

## Success Metrics
- [ ] Code coverage improved by X%
- [ ] Performance improved by X%
- [ ] Reduced complexity in module Y
- [ ] Dependencies updated to latest versions
